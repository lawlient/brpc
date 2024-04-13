import "primereact/resources/themes/lara-light-cyan/theme.css";
import React from 'react'
import * as ReactDOM from 'react-dom/client'
import { createBrowserRouter, RouterProvider } from 'react-router-dom';
import App from './App.jsx'
import Setting from './pages/Setting.jsx';
import Root from './Root.jsx';
import Tasks from './pages/Tasks.jsx';
import Disk from "./pages/Disk.jsx";
import User from "./pages/User.jsx";
import Signin from "./pages/Signin.jsx";


const router = createBrowserRouter([
  {
    path: '/xpansyncui/',
    element: <App />,
    children: [
      {
        path: "",
        element: <Root />,
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
        path: "auth/signin",
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
