# add_executable(box_run run.cpp Box.cpp)
add_executable(sir_simulator_gui gui.cpp Box.cpp Simulator.cpp)
target_link_libraries(sir_simulator_gui PUBLIC Qt6::Widgets Qt6::Charts)
qt_add_executable(Server
    Server.cpp Server.h
    main.cpp
)
target_link_libraries(Server PUBLIC Qt::WebSockets)

