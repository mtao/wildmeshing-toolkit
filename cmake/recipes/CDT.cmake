# VolumeMesher (From Marco Attene)

if(TARGET CDT::CDT)
    return()
endif()

message(STATUS "Third-party: creating target 'CDT'")

include(CPM)
#CPMAddPackage("gh:JcDai/CDT#9f6c6e1588f1f4877692629ee38913248aebfd55")
CPMAddPackage("gh:wildmeshing/CDT#ddf0af84289bedae5a711be05bfc941d2f6f4ee4")

set_target_properties(cdt_lib PROPERTIES FOLDER third-party)

