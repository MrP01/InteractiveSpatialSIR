import * as M from "@materializecss/materialize";
import "@materializecss/materialize/dist/css/materialize.css";
import "./App.css";

import { Joystick } from "react-joystick-component";

import React from "react";
import { Button, TextInput } from "react-materialize";

var socket = null;
function connectSocket() {
  socket = new WebSocket(`ws://${document.location.hostname}:1234`);
  socket.onopen = () => {
    console.log("WebSocket Client Connected");
  };
}
connectSocket();

function handleMove(event) {
  if (socket.readyState === WebSocket.OPEN) {
    socket.send(event.x / 3 + ";" + event.y / 3);
  } else if (socket.readyState === WebSocket.CLOSED) {
    connectSocket();
  }
}
function handleStop() {
  socket.send("0;0");
}

class App extends React.Component {
  constructor() {
    super();
    this.nicknameInputRef = React.createRef();
    this.state = {
      confirmedNickname: "Peter",
    };
  }

  render() {
    return (
      <div className="App">
        {/* <img src={logo} className="App-logo" alt="logo" /> */}
        <h1>Stay Healthy!</h1>
        <div className={this.state.confirmedNickname !== null ? "hidden" : ""}>
          <TextInput name="name" ref={this.nicknameInputRef} label="Name" />
          <Button
            onClick={() => {
              if (this.nicknameInputRef.current.value.length > 1) {
                M.toast({ text: "Let's go: control your player by moving the JoyStick!" });
                this.setState({ confirmedNickname: this.nicknameInputRef.current.value });
              } else {
                M.toast({ text: "Please enter a value with at least 2 characters!" });
              }
            }}
          >
            Set Nickname
          </Button>
        </div>
        <div className={this.state.confirmedNickname === null ? "hidden" : ""}>
          <Joystick size={200} baseColor="black" stickColor="blue" move={handleMove} stop={handleStop}></Joystick>
        </div>
      </div>
    );
  }
}

export default App;
