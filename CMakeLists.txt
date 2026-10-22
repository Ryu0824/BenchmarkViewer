cmake_minimum_required(VERSION 4.2.0)
project(BenchmarkViewer VERSION 1.0.0 LANGUAGES CXX)

function(configure_target target)
	if(MSVC)
		target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8 /EHsc)
		set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	else()
		target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
	endif()
endfunction()

if(WIN32)
	add_executable(BenchmarkViewer WIN32 
		src/WinMain.cpp 
		src/Window/WindowClass.cpp
		src/Viewer/Viewer.cpp
		src/Window/Object/Object.cpp
		src/Window/Component/Component.cpp
		src/Window/Component/Transform.cpp
		src/Window/Component/Button.cpp
		src/Window/Component/ListBox.cpp
		src/Window/Component/TextBox.cpp
		src/Window/Component/ListView.cpp
		src/Window/Component/BarChart.cpp
		src/Window/Component/ChartGroup.cpp
		src/Window/Manager/InputManager.cpp
		src/Window/Manager/ConsoleManager.cpp
		src/Window/Manager/RenderManager.cpp
		src/Window/Manager/TimeManager.cpp
		src/Window/Manager/RenderManager.cpp
		src/Window/Manager/EventManager.cpp)

	target_compile_definitions(BenchmarkViewer PRIVATE UNICODE _UNICODE)
	target_link_libraries(BenchmarkViewer PRIVATE BenchmarkData user32 gdi32 comdlg32 comctl32 shell32)
	configure_target(BenchmarkViewer)
	if(MINGW)
		target_link_options(BenchmarkViewer PRIVATE -municode)
	endif()
	install(TARGETS BenchmarkViewer RUNTIME DESTINATION .)
	target_include_directories(BenchmarkViewer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
else()
	message(STATUS "Win32 UI is disabled on this platform; data/scale tests remain available".)
endif() 

include(CTest)
if(BUILD_TESTING)
	add_executable(BenchmarkDataTests tests/BenchmarkDataTests.cpp)
	target_link_libraries(BenchmarkDataTests PRIVATE BenchmarkData)
	target_include_directories(BenchmarkDataTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
	configure_target(BenchmarkDataTests)
	add_test(NAME BenchmarkDataTests COMMAND BenchmarkDataTests "${CMAKE_CURRENT_SOURCE_DIR}/examples")
endif()

add_subdirectory("src/BenchmarkData")