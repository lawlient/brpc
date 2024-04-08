import { Outlet } from 'react-router-dom'
import { Menu } from 'primereact/menu';
import {useNavigate} from 'react-router-dom'


import 'primeicons/primeicons.css';
import './Root.css';


export default function Root() {
  const nav = useNavigate()

  const navs = [
    {
      label: '网盘',
      icon: () => <i className='pi pi-cloud' ></i>,
      command: () => {
        nav("/disk")
      }
    },
    {
      label: '任务',
      icon: () => <i className='pi pi-list' style={{ color: 'gray'}} ></i>,
      command: () => {
        nav("/tasks")
      }
    },
    {
      label: '设置',
      icon: () => <i className='pi pi-cog' style={{ color: 'gray'}} ></i>,
      command: () => {
        nav("/setting")
      }
    },
    {
      label: '管理员',
      icon: () => <i className='pi pi-user' ></i>,
      command: () => {
        nav("/admin")
      }
    },
  ]

  return (
    <div className='root'>
      <div className='layout'>
        <div className='sidebar' >
          <div className='sidebar-header'>xpansync</div>
          <Menu model={navs} position='left' />
        </div>
        <main className='content'>
          <Outlet />
        </main>
      </div>
    </div>
  )
}

