import unittest

import sys
import os
import platform
import json
import subprocess
import tempfile
import argparse

def fix_path(path):
    cwd = os.getcwd()
    return path if os.path.isabs(path) else os.path.join(cwd, path)



class IntegrationTest(unittest.TestCase):
    BINARY_FOLDER = fix_path(os.environ['WMTK_BINARY_FOLDER']) if "WMTK_BINARY_FOLDER" in os.environ else None
    CONFIG_FILE = fix_path(os.environ['WMTK_CONFIG_FILE']) if "WMTK_CONFIG_FILE" in os.environ else None

    # verbose = print stdout / stderr to the screen
    def __init__(self, name, test_config, configs_to_run = None, run_all = False, verbose=False):
        super().__init__()
        self.working_dir_fp = tempfile.TemporaryDirectory()
        self.working_dir = self.working_dir_fp.name
        self.verbose= verbose

        self.name = name
        self.test_config = test_config
        self.run_all = run_all

        self.data_folder = None if "data_folder" not in test_config else test_config["data_folder"]
        if self.data_folder is None:
            self.data_folder = None if "input_directory_tag" not in test_config else test_config["input_directory_tag"]
        print(f"Data folder {self.data_folder}")

        self.config_folder = self.data_folder if "config_folder" not in test_config else test_config["config_folder"]
        msg = 'all (slow and fast)' if self.run_all else 'fast'
        print(f'Loading integration test [{name}] in {self.working_dir}, running {msg} tests', flush=True)


        file = test_config["config_file"]

        with open(file) as fp:
            config = json.load(fp)
        self.config = config

        if configs_to_run is not None:
            self.config["tests"] = configs_to_run

        if "test_directory" in config:
            self.config_folder = os.path.join(self.config_folder, config["test_directory"])
            if "test_data_directory" not in config:
                # some applications assume taht these follow together
                self.data_folder = os.path.join(self.data_folder, config["test_directory"])
        if "test_data_directory" in config:
            self.data_folder = os.path.join(self.data_folder, config["test_directory"])



        self.executable = os.path.abspath(os.path.join(IntegrationTest.BINARY_FOLDER, self.name))

        self.extra_flags = test_config["extra_flags"].split()



    def tearDown(self):
        self.working_dir_fp.cleanup()


    def execute_json(self, json_file_path):

        cmd = [self.executable] +self.extra_flags + [ "-j", json_file_path]
        # capture input if we're not going to already print stuff
        res = subprocess.run(cmd, cwd=self.working_dir, capture_output=not self.verbose)

        if res.returncode != 0:
            print(f"Error running [{' '.join(cmd)}] from working directory [{self.working_dir}]")
            # in verbose mode the stderr/stdout buffers were already flushed
            if not self.verbose:
                print(res.stderr.decode('utf-8'), flush=True)
                print(res.stdout.decode('utf-8'), flush=True)
        return res

    def get_root_path(self, input_js):
        root_tag = self.config["input_directory_tag"]
        if root_tag in input_js:
            if not os.path.isabs(input_js[root_tag]):

                return os.path.join(self.data_folder, input_js[root_tag])
        
        return self.data_folder


    def create_reporter(self, input_json_file, output_json_file):

        # load the input json
        with open(input_json_file) as f:
            input_js = json.load(f)


        # prepare it with reporter data
        oracle_tag = self.config["oracle_tag"]
        root_tag = self.config["input_directory_tag"]

        input_js[oracle_tag] = os.path.abspath(os.path.join(self.config_folder, output_json_file))
        input_js[root_tag ] = self.get_root_path(input_js)

        with tempfile.NamedTemporaryFile(mode='w', delete=False) as input_json:
            json.dump(input_js, input_json)
            input_json.file.close()

            res = self.execute_json(input_json.name)

        if res.returncode != 0:
            print(res)

        assert(res.returncode == 0)




    def run_one(self, test_file):
        self.assertTrue(os.path.exists(test_file), f"{test_file} does not exist")

        input_tag = self.config["input_tag"]
        oracle_tag = self.config["oracle_tag"]
        root_tag = self.config["input_directory_tag"]

        has_checks = "checks" in self.config
        checks = [] if not has_checks else self.config["checks"]





        with open(test_file) as f:
            try:
                test_oracle = json.load(f)
                f.close()
            except Exception as e:
                print(f"Caught exception while loading file {test_file}: {e}", flush=True)
                raise e

        input_js = test_oracle[input_tag].copy()
        with tempfile.NamedTemporaryFile(mode='r', delete=True) as oracle_file:
            oracle_file.file.close()

            input_js[oracle_tag] = oracle_file.name

                
            input_js[root_tag] = self.get_root_path(input_js)

            with tempfile.NamedTemporaryFile(mode='w', delete=False) as input_json:
                json.dump(input_js, input_json)
                input_json.file.close()


                res = self.execute_json(input_json.name)

                self.assertEqual(res.returncode, 0, f"{res.returncode} != 0")


                with open(oracle_file.name, "r") as fp:
                    result = json.load(fp)

            for check in checks:
                self.assertEqual(result[check], test_oracle[check], f"{result[check]} != {test_oracle[check]}")

            if len(checks) == 0 and not has_checks:
                for k in test_oracle:
                    if k == input_tag:
                        continue

                    self.assertTrue(k in result)
                    self.assertEqual(result[k], test_oracle[k], f"{result[k]} != {test_oracle[k]}")


        self.assertTrue(True)

    def runTestFile(self, test_file_name):
        with self.subTest(msg=f"{self.name}-{test_file_name}"):
            print("Running test", test_file_name, flush=True)
            test_file = os.path.join(self.config_folder, test_file_name)
            print(f"Test file: {test_file}", flush=True)
            self.run_one(test_file)


    def runTest(self):
        for test_file_name in self.config["tests"]:
            self.runTestFile(test_file_name)
        if self.run_all and "release_only_tests" in self.config:
            for test_file_name in self.config["release_only_tests"]:
                self.runTestFile(test_file_name)


def load_config_json(config_file):
    with open(config_file) as fp:
        config = json.load(fp)

        if "skip" in config:
            del config["skip"]
    return config

def make_suite(config_file, single_application = None, single_config = None, run_all = False, verbose=False):
    config = load_config_json(config_file)

    suite = unittest.TestSuite()
    for key,value in config.items():
        if "platform" in value and value["platform"] != "" and value["platform"] != platform.system():
            print(f"Skipping checks for application {key} because the platform is {platform.system()} and the test is for {config['platform']}", flush=True)
            continue
        if single_application is None or key == single_application:
            # expects a list of configs to run
            suite.addTest(IntegrationTest(key,value, None if single_config is None else [single_config], run_all, verbose=verbose))
    return suite




if __name__ == '__main__':
    parser = argparse.ArgumentParser(
                    prog='WMTK Integration Test',
                    description='Run integration tests for WMTK')

    parser.add_argument('-c', '--test_config', help="Path to the json config file for integration testing parameters")
    parser.add_argument('-b', '--binary_folder', help="Path to the folder that contains the apps binaries")
    parser.add_argument('-t', '--test-application', help="Runs tests for a single application")
    parser.add_argument('-s', '--test-script', help="Runs a particular test script")
    parser.add_argument('-a', '--all-tests', help="Runs all tests, including slow ones", action='store_true')
    parser.add_argument("-v", '--verbose', help="print execution data verbosely", action='store_true')

    subparsers = parser.add_subparsers(help="subcommand help", dest="subcommand")
    create_parser = subparsers.add_parser(name="create",help="create integration test json")

    create_parser.add_argument("-b", '--binary', help="Name of the binary being run")
    create_parser.add_argument("-i", '--input', help="input config")
    create_parser.add_argument("-o", '--output', help="output config filename, placed in config folder")
    create_parser.add_argument("-v", '--verbose', help="print execution data verbosely", action='store_true')

    args = parser.parse_args()



    tcin = args.test_config
    bfin = args.binary_folder

    cwd = os.getcwd()
    if tcin:
        config_file = fix_path(tcin)
    else:
        config_file = os.path.join(cwd, "test_config.json")


    if bfin:
        bin_dir = fix_path(bfin)
    else:
        bin_dir = os.path.join(cwd, "applications")




    IntegrationTest.BINARY_FOLDER = bin_dir
    IntegrationTest.CONFIG_FILE = config_file



    # no subcommand chosen so we just run
    if args.subcommand is None:
        # test_application and test_script are None if not set
        suite = make_suite(config_file, args.test_application, args.test_script, args.all_tests, verbose=args.verbose)

        runner = unittest.TextTestRunner()
        result = runner.run(suite)
        if len(result.errors) > 0 or len(result.failures) > 0:
            for s,error in result.errors:
                print("While running: ", s, flush=True)
                print(error, flush=True)
            for s,failure in result.failures:
                print("While running: ", s, flush=True)
                print(failure, flush=True)
            assert(len(result.errors) == 0)
            assert(len(result.failures) == 0)
    elif args.subcommand == "create":
        config = load_config_json(config_file)
        binary = args.binary
        my_config = config[binary]
        test = IntegrationTest(binary,my_config, verbose=args.verbose)
        test.create_reporter(args.input,args.output)
        pass


