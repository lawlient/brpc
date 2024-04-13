import './Root.css';
import 'primeicons/primeicons.css';
import { Outlet } from 'react-router-dom'
import { useEffect, useRef, useState } from 'react';
import { Menu } from 'primereact/menu';
import { useNavigate } from 'react-router-dom'
import { Avatar } from 'primereact/avatar';
import { Divider } from 'primereact/divider';


export default function Root() {
  const n = useNavigate()
  const nav = (p) => {
    n(import.meta.env.BASE_URL+p)
  }

  const [logo] = useState("")

  const sidebar = useRef()
  const content = useRef()
  useEffect(() => {
    let path = window.location.href
    let isroot = true;
    for (let index = 0; index < navs.length; index++) {
      let node = document.getElementById(navs[index].to)
      if (path.endsWith(navs[index].to)) {
        node.style.backgroundColor = 'var(--surface-200)';
        isroot = false;
      } else {
        node.style.backgroundColor = "white";     
      }
    }
    if (isroot) {
      let node = document.getElementById(navs[0].to)
      node.style.backgroundColor = 'var(--surface-200)';
    }
  }, [window.location.href])


  const navtemp = (e) => {
    return (
      <div className='menuitem' id={e.to}>
        <a className="menuitem-content" onClick={() => { nav(e.to) }}>
          <i className={e.icon} ></i>
          <span >{e.label}</span>
        </a>
      </div>
    )
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

  const toggleSidebar = () => {
    sidebar.current.classList.toggle("toggle-sidebar")
    content.current.classList.toggle("toggle-content")
  }

  return (
    <>
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
          <button className="toggle-button" onClick={toggleSidebar} ><i className='pi pi-bars' ></i></button>
          <Avatar image="" />
        </div>
        <div>
          <Outlet />
        </div>
      </main>
    </>
  )
}

