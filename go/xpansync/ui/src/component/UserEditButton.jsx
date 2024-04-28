import "./UserEditButton.css"
import { useState } from "react";
import { Avatar } from "primereact/avatar";
import { Dialog } from "primereact/dialog";
import { InputText } from "primereact/inputtext";
import { Button } from "primereact/button";
import { upduser } from "../api/api";

export function UserEditButton({lists, setLists, idx}) {
    const [visib, setVisib] = useState(false)
    const [user, setUser] = useState(lists[idx])

    const submit = (e) => {
        e.preventDefault()
        upduser(user).then(res => {
            let _tmp = [...lists]
            _tmp[idx] = user
            setLists(_tmp)
        })

        setVisib(false)
    }

    return (
        <>
        <button onClick={() => setVisib(true)}><i className="pi pi-pencil"></i></button>
        <Dialog visible={visib} onHide={() => setVisib(false)}>
            <form className="user-edit-dialog"  onSubmit={submit} >
                <Avatar image={user.avatar} />
                <InputText type="text" value={user.avatar} onChange={(e) => setUser({...user, avatar:e.target.value})} />
                <InputText value={user.username} onChange={(e) => setUser({...user, username:e.target.value})} />
                <InputText type="password" value={user.password} onChange={(e) => setUser({...user, password:e.target.value})} />
                <Button type="submit" >Submit</Button>
            </form>
        </Dialog>
        </>
    )
}