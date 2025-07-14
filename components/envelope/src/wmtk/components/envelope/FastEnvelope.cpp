
bool BVHEnvelope::after(
    const std::vector<Tuple>& top_dimension_tuples_before,
    const std::vector<Tuple>& top_dimension_tuples_after) const
{
    if (top_dimension_tuples_after.empty()) return true;

    assert(m_coordinate_handle.holds<Rational>() || m_coordinate_handle.holds<double>());

    if (m_coordinate_handle.holds<Rational>()) {
        const attribute::Accessor<Rational> accessor =
            mesh().create_const_accessor(m_coordinate_handle.as<Rational>());
        const PrimitiveType type = mesh().top_simplex_type();

        if (m_envelope) {
            assert(accessor.dimension() == 3);

            std::vector<Tuple> faces;

            if (type == PrimitiveType::Triangle) {
                std::array<Eigen::Vector3d, 3> triangle;

                for (const Tuple& tuple : top_dimension_tuples_after) {
                    faces = faces_single_dimension_tuples(
                        mesh(),
                        simplex::Simplex(mesh(), type, tuple),
                        PrimitiveType::Vertex);

                    triangle[0] = accessor.const_vector_attribute(faces[0]).cast<double>();
                    triangle[1] = accessor.const_vector_attribute(faces[1]).cast<double>();
                    triangle[2] = accessor.const_vector_attribute(faces[2]).cast<double>();

                    if (m_envelope->is_outside(triangle)) {
                        wmtk::logger().debug("fail envelope check 1");
                        return false;
                    }
                }

                return true;
            } else if (type == PrimitiveType::Edge) {
                for (const Tuple& tuple : top_dimension_tuples_after) {
                    faces = faces_single_dimension_tuples(
                        mesh(),
                        simplex::Simplex(mesh(), type, tuple),
                        PrimitiveType::Vertex);

                    Eigen::Vector3d p0 = accessor.const_vector_attribute(faces[0]).cast<double>();
                    Eigen::Vector3d p1 = accessor.const_vector_attribute(faces[1]).cast<double>();

                    if (m_envelope->is_outside(p0, p1)) {
                        wmtk::logger().debug("fail envelope check 2");
                        return false;
                    }
                }

                return true;
            } else if (type == PrimitiveType::Vertex) {
                for (const Tuple& tuple : top_dimension_tuples_after) {
                    Eigen::Vector3d p = accessor.const_vector_attribute(tuple).cast<double>();

                    if (m_envelope->is_outside(p)) {
                        wmtk::logger().debug("fail envelope check 3");
                        return false;
                    }
                }

                return true;
            } else {
                throw std::runtime_error("Invalid mesh type");
            }
            return true;
        } else if (m_coordinate_handle.holds<double>()) {
            const attribute::Accessor<double> accessor =
                mesh().create_const_accessor(m_coordinate_handle.as<double>());
            const auto type = mesh().top_simplex_type();

            if (m_envelope) {
                assert(accessor.dimension() == 3);

                std::vector<Tuple> faces;

                if (type == PrimitiveType::Triangle) {
                    std::array<Eigen::Vector3d, 3> triangle;

                    for (const Tuple& tuple : top_dimension_tuples_after) {
                        faces = faces_single_dimension_tuples(
                            mesh(),
                            simplex::Simplex(mesh(), type, tuple),
                            PrimitiveType::Vertex);

                        triangle[0] = accessor.const_vector_attribute(faces[0]);
                        triangle[1] = accessor.const_vector_attribute(faces[1]);
                        triangle[2] = accessor.const_vector_attribute(faces[2]);

                        if (m_envelope->is_outside(triangle)) {
                            wmtk::logger().debug("fail envelope check 7");
                            return false;
                        }
                    }

                    return true;
                } else if (type == PrimitiveType::Edge) {
                    for (const Tuple& tuple : top_dimension_tuples_after) {
                        faces = faces_single_dimension_tuples(
                            mesh(),
                            simplex::Simplex(mesh(), type, tuple),
                            PrimitiveType::Vertex);

                        Eigen::Vector3d p0 = accessor.const_vector_attribute(faces[0]);
                        Eigen::Vector3d p1 = accessor.const_vector_attribute(faces[1]);

                        if (m_envelope->is_outside(p0, p1)) {
                            wmtk::logger().debug("fail envelope check 8");
                            return false;
                        }
                    }

                    return true;
                } else if (type == PrimitiveType::Vertex) {
                    for (const Tuple& tuple : top_dimension_tuples_after) {
                        Eigen::Vector3d p = accessor.const_vector_attribute(tuple);

                        if (m_envelope->is_outside(p)) {
                            wmtk::logger().debug("fail envelope check 9");
                            return false;
                        }
                    }

                    return true;
                } else {
                    throw std::runtime_error("Invalid mesh type");
                }
                return true;
            }
        } else {
            throw std::runtime_error("Envelope mesh handle type invlid");
        }
    }
