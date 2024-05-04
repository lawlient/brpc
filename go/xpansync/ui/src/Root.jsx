import './Root.css';
import 'primeicons/primeicons.css';
import { Outlet } from 'react-router-dom'
import { useEffect, useRef, useState } from 'react';
import { Menu } from 'primereact/menu';
import { useNavigate } from 'react-router-dom'
import { Avatar } from 'primereact/avatar';


export default function Root() {
  const n = useNavigate()
  const nav = (p) => { n(import.meta.env.BASE_URL+p) }

  const [logo] = useState("")

  const layout = useRef()
  const sidebar = useRef()
  const content = useRef()
  const navtemp = (e) => {
    return focus === e.to ? 
      <div className='menuitem' id={e.to}>
        <a className="menuitem-content-selected menuitem-content" onClick={() => { nav(e.to) }}>
          <i className={e.icon} ></i>
          <span >{e.label}</span>
        </a>
      </div>
      : 
      <div className='menuitem' id={e.to}>
        <a className="menuitem-content" onClick={() => { nav(e.to) }}>
          <i className={e.icon} ></i>
          <span >{e.label}</span>
        </a>
      </div>
  }

  const navs = [
    {
      label: '网盘',
      icon: 'pi pi-cloud',
      to: "disk",
      template: navtemp,
    },
    {
      label: '任务',
      icon: 'pi pi-list',
      to: "tasks",
      template: navtemp,
    },
    {
      label: '设置',
      icon: 'pi pi-cog',
      to: "setting",
      template: navtemp,
    },
    {
      label: '管理员',
      icon: 'pi pi-user',
      to: "admin",
      template: navtemp,
    },
  ]
  const [focus, setFocus] = useState(navs[0].to)

  const toggleSidebar = () => {
    sidebar.current.classList.toggle("toggle-sidebar")
    content.current.classList.toggle("toggle-content")
  }

  const toggleScheme = () => {
    layout.current.classList.toggle('dark')
  }

  useEffect(() => {
    for (var index = 0; index < navs.length; index++) {
      if (window.location.href.endsWith(navs[index].to)) {
        setFocus(navs[index].to)
        break
      }
    }
  }, [window.location.href])


  return (
    <div className='layout' ref={layout}>
      <div className='sidebar' ref={sidebar}>
        <div className='sidebar-head'>
          <img src={logo || import.meta.env.BASE_URL+"/public/logo.svg"} alt="logo" />
          <div>xpansync</div>
        </div>
        <hr  style={{margin:'0 0 20px', borderTop:'1px solid black'}}/>
        <Menu className="menu" model={navs} />
      </div>
      <main className='content' ref={content}>
        <div className='content-head'>
          <button onClick={toggleSidebar} ><i className='pi pi-bars' ></i></button>
          <button onClick={toggleScheme} ><i className='pi pi-moon' ></i></button>
          <Avatar shape="circle" icon="pi pi-user" image={localStorage.getItem("__avatar__")} />
        </div>
        <div>
          <Outlet />
        </div>
      </main>
    </div>
  )
}

