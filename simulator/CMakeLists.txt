add_executable(box_run run.cpp Box.cpp)
add_executable(box_gui gui.cpp Box.cpp Simulator.cpp)
target_link_libraries(box_gui PUBLIC Qt6::Widgets Qt6::Charts)
