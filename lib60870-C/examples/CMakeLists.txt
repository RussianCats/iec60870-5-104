
add_subdirectory(cs104_client)
add_subdirectory(cs104_server)


if (WITH_MBEDTLS)
add_subdirectory(tls_client)
add_subdirectory(tls_server)
endif (WITH_MBEDTLS)

