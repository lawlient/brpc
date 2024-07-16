import './Root.css';
import 'primeicons/primeicons.css';
import { Outlet } from 'react-router-dom'
import { useEffect, useRef, useState } from 'react';
import { useNavigate } from 'react-router-dom'


export default function Root() {
  const n = useNavigate()
  const nav = (p) => { n(import.meta.env.BASE_URL+p) }

  const [logo] = useState("")
  const [dark, setDark] = useState(false)

  const layout = useRef()
  const sidebar = useRef()
  const content = useRef()
  const navtemp = (e, i) => {
    return (
      <a key={i} className={`${focus === e.to && "menuitem-selected"} menuitem`}
        onClick={() => { nav(e.to) }}>
        <i className={e.icon} ></i>
        <span >{e.label}</span>
      </a>
    )
  }

  const navs = [
    {
      label: '网盘',
      icon: 'pi pi-cloud',
      to: "disk",
    },
    {
      label: '任务',
      icon: 'pi pi-list',
      to: "tasks",
    },
    {
      label: '设置',
      icon: 'pi pi-cog',
      to: "setting",
    },
    {
      label: '管理员',
      icon: 'pi pi-user',
      to: "admin",
    },
  ]
  const [focus, setFocus] = useState(navs[0].to)

  const toggleSidebar = () => {
    sidebar.current.classList.toggle("toggle-sidebar")
    content.current.classList.toggle("toggle-content")
  }

  const toggleScheme = () => {
    layout.current.classList.toggle('dark')
    setDark(!dark)
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
          <img src={logo || import.meta.env.BASE_URL+"logo.svg"} alt="logo" />
          <div>xpansync</div>
        </div>
        <nav className="menu">
          {navs.map((n, i) => {
            return navtemp(n, i)
          })} 
        </nav> 
      </div>

      <main className='content' ref={content}>
        <div>
          <Outlet />
        </div>
        <button className="sidebar-button" onClick={toggleSidebar} >
          <i className='pi pi-bars' ></i>
        </button>
        <button className="theme-button" onClick={toggleScheme} >
          {dark ? <i className='pi pi-moon' ></i> : <i className='pi pi-sun' ></i> }
        </button>
      </main>
    </div>
  )
}

