import "./User.css"
import { Card } from "primereact/card";
import { useEffect, useState } from "react";
import { userlist } from "../api/api";
import { DataView } from "primereact/dataview";
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

    const userTemp = (en, idx) => {
        return (
            <div className="user-item">
                <Avatar image={en.avatar} shape="circle"/>
                <h3>{en.username}</h3>
                <Tag color="primary" value={en.role} />
                <UserEditButton lists={list} setLists={setList} idx={idx} />
            </div>
        )
    }

    return (
        <div className="user-list">
        {
            list.map((en, idx) => {
                return userTemp(en, idx)
            })
        }
        </div>
    )
}