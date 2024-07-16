import "./User.css"
import { useEffect, useState } from "react";
import { userlist } from "../api/api";
import { Avatar } from "primereact/avatar";
import { Tag } from "primereact/tag";
import { UserEditButton } from "../component/UserEditButton";

export default function User() {
  const [list, setList] = useState([])
  useEffect(() => {
    userlist().then(res => {
      setList(res.data.data)
    })
  }, [])

  return (
    <div className="user-container">
      {
        list.map((en, idx) => {
          return (
            <div key={idx} className="user-card">
              <Avatar image={en.avatar} shape="circle" />
              <h3>{en.username}</h3>
              <Tag severity="success" value={en.role} />
              <UserEditButton lists={list} setLists={setList} idx={idx} />
            </div>
          )
        })
      }
    </div>
  )
}