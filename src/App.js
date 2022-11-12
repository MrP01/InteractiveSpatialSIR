import logo from './logo.svg';
import './App.css';
import { Joystick } from 'react-joystick-component';

const socket = new WebSocket("wss://localhost:1234");
/*socket.onopen = (event) => {
  socket.send("uhhh random");
};*/
function handleMove(event) {
  console.log("Move", event);
}
function handleStop() {
  console.log("Stop");
}


function App() {
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <h1>Test!</h1>
        <Joystick size={100} baseColor="black" stickColor="blue" move={handleMove} stop={handleStop}></Joystick>
      </header>
    </div>
  );
}

export default App;
