
add_subdirectory(cs104_client)
add_subdirectory(cs104_server)
# add_subdirectory(generator)


if (WITH_MBEDTLS)
add_subdirectory(tls_client)
add_subdirectory(tls_server)
# add_subdirectory(generator)
endif (WITH_MBEDTLS)

