import React from 'react';
import { Link, Route, BrowserRouter as Router, Routes } from "react-router-dom";
import Login from "./pages/Login/App"
import Admin from "./pages/Admin/App"
import Main from "./layout/Main/index"



const Home = () => {
    return (
      <Link to={"/login"}>Login</Link>
    )
}


const NoPage = () => {
  return (
    <h3>404</h3>
  )
}


const App = () => {
    return (
      <Router basename='mlul'>
        <Routes>
            <Route path="/" element={ <Home /> } />
            <Route path="/login" element={<Login />} />
            <Route path="/admin" element={ 
              <Main >
                <Admin />
              </Main>
            } />
            <Route path="*" element={<NoPage />} />
        </Routes>
      </Router>
    )
};
export default App;
