import { Card } from "primereact/card";
import { Avatar } from 'primereact/avatar'
import { useEffect, useState } from "react";
import { getcode, newtoken, gettoken, userinfo, getquota } from "../api/api";
import { Button } from "primereact/button";
import { InputText } from "primereact/inputtext";
import { Panel } from "primereact/panel"



export default function Disk() {
    const [user, setUser] = useState({})
    const [quota, setQuota] = useState({})

    const toGB = (b) => { return Math.ceil(b / (1024 * 1024 * 1024)) }

    useEffect(() => {
        userinfo().then(res => {
            setUser(res.data.data)
        })
    }, [])

    useEffect(() => {
        gettoken().then(res => {
            setToken(res.data.data)
        })
    }, [])

    useEffect(() => {
        getquota().then(res => {
            setQuota(res.data.data)
            console.log(res.data.data)
        })
    }, [])

    const [code, setCode] = useState("")
    const [token, setToken] = useState("")

    return (
        <Card >
            <Avatar image={user.avatar_url} shape="Badge" size="xlarge"/>
            <p>{user.baidu_name}</p>
            <p>{toGB(quota.used)} / {toGB(quota.total)}  GB</p>
            <Panel header="Token">
                <p>{token}</p>
            </Panel>
            <Button onClick={() => {
                getcode().then(res => {
                    console.log(res.data)
                    window.open(res.data.data)
                })
            }}>Get Code</Button>

            <InputText value={code} onChange={(e) => setCode(e.target.value)} placeholder="Code" />

            <Button onClick={() => {
                newtoken(code).then(res => {
                    setToken(res.data.data) 
                    setCode("")
                })
            }}>Token</Button>

        </Card>
    )
}