import "./Disk.css"
import { Avatar } from 'primereact/avatar'
import { useEffect, useState } from "react";
import { userinfo, getquota } from "../api/api";
import { TokenPanel } from "../component/TokenPanel";



export default function Disk() {
  const [user, setUser] = useState({})
  const [quota, setQuota] = useState({})

  const toGB = (b) => { return Math.ceil(b / (1024 * 1024 * 1024)) }
  const barwidth = (quota) => {
    return Math.floor(quota.used/quota.total * 100).toString() + "%" 
  }

  useEffect(() => {
    userinfo().then(res => {
      setUser(res.data.data)
    })
  }, [])

  useEffect(() => {
    getquota().then(res => {
      setQuota( res.data.data)
      console.log( res.data.data)
    })
    console.log(quota)
  }, [])

  return (
    <div className="diskcard">
      <Avatar className="avatar" image={user.avatar_url} shape="circle" size="xlarge" />
      <h3 style={{margin:'4px'}}>{user.baidu_name}</h3>
      <div className="diskspace">
        <div className="total">
          <div style={{width: barwidth(quota)}} className="used">
          </div>
        </div>
        <p>{toGB(quota.used)}GB/{toGB(quota.total)}GB {barwidth(quota)}</p>
      </div>

      <TokenPanel />
    </div>
  )
}