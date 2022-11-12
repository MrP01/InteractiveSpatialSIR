import * as M from "@materializecss/materialize";
import "@materializecss/materialize/dist/css/materialize.css";
import "./App.css";

import { Joystick } from "react-joystick-component";

import React from "react";
import { Button, TextInput } from "react-materialize";

//const socket = new WebSocket("wss://localhost:1234");
/*socket.onopen = (event) => {
  socket.send("uhhh random");
};*/

function handleMove(event) {
  // console.log("Move", event);
}
function handleStop() {
  // console.log("Stop");
}

class App extends React.Component {
  constructor() {
    super();
    this.nicknameInputRef = React.createRef();
    this.state = {
      confirmedNickname: null,
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
