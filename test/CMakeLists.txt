add_executable(
	cpp_bind_test
	main.cpp
)

target_link_libraries(
	cpp_bind_test
	PRIVATE
	llhttp++
)
