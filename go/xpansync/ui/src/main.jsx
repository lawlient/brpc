import "primereact/resources/themes/lara-light-cyan/theme.css";
import React from 'react'
import * as ReactDOM from 'react-dom/client'
import { createBrowserRouter, RouterProvider, redirect } from 'react-router-dom';
import App from './App.jsx'
import Setting from './pages/Setting.jsx';
import Root from './Root.jsx';
import Tasks from './pages/Tasks.jsx';
import Disk from "./pages/Disk.jsx";
import User from "./pages/User.jsx";
import Signin from "./pages/Signin.jsx";
import { getuser } from "./api/api.js";

// loader, must return something
const init_user = async () => {
    var username = localStorage.getItem("__username__")
    if (username == null || username === "") {
        redirect(import.meta.env.BASE_URL + "signin")
        return null
    }   
    console.log("Welcome to xpansync")
    await getuser({username: username}).then(res => {
        var me = res.data.data[0]
        localStorage.setItem("__avatar__", me.avatar)
    })  
    return null
}

const router = createBrowserRouter([
  {
    path: import.meta.env.BASE_URL,
    element: <App />,
    children: [
      {
        path: "",
        element: <Root />,
        loader: () => init_user(),
        children: [
          {
            path: "",
            element: <Disk />,
          },
          {
            path: "disk",
            element: <Disk />,
          },
          {
            path: "tasks",
            element: <Tasks />
          },
          {
            path: "setting",
            element: <Setting />
          },
          {
            path: "admin",
            element: <User />
          },
        ]
      },
      {
        path: "signin",
        element: <Signin />,
      },
    ]
  }
]);

ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <RouterProvider router={router} />
  </React.StrictMode>,
)
