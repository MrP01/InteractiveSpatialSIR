# add_executable(box_run run.cpp Box.cpp)
add_executable(sir_simulator_gui gui.cpp Box.cpp Simulator.cpp Server.cpp SimulatorThread.cpp)
target_link_libraries(sir_simulator_gui PUBLIC Qt6::Widgets Qt6::Charts Qt::WebSockets)
#qt_add_executable(Server Server.cpp Server.hmain.cpp)
#target_link_libraries(Server PUBLIC Qt::WebSockets)
