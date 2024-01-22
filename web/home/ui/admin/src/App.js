import logo from './logo.svg';
import './App.css';


function OfficialApp() {
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Edit <code>src/App.js</code> and save to reload.
        </p>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
      </header>
    </div>
  );
}

function Head() {
  return (
    <div className='head'>
      <h1 align="center">Jan</h1>
    </div>
  );
}

function Entry({entry}) {
  return (
    <div id={entry.id}>
      <h3>{entry.title}</h3>
      <p>{entry.description}</p>
      <a href={entry.ur} display="none"></a>
    </div>
  );
}

function EntryList({entrances}) {
  const lists = entrances.map(entry =>
    <Entry entry={entry} />
  );
  return (
    <div id="entrances">
      {lists}
    </div>
  );
}

const entry_list = [
  {id : 1, title: "FTP", description: "FTP Server", url:"/ftp"},
  {id : 2, title: "Music", description: "Music App", url:"/music"},
];

function App() {
  return (
    <div id="home">
      <Head />
      <EntryList entrances={entry_list} />
    </div>

  );
}

export default App;
