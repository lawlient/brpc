import "./Signin.css"
import { useEffect, useState } from "react"
import { signin, signup, usize } from "../api/api"
import { InputText } from "primereact/inputtext"
import { Button } from "primereact/button"
import axios from "axios"
import { useNavigate } from "react-router-dom"


export default function Signin() {
    const [info, setInfo] = useState({
        "username": "",
        "password": "",
    })
    const [logo] = useState(import.meta.env.BASE_URL + "public/vite.svg")
    const nav = useNavigate()

    const [first, setFirst] = useState(false)
    useEffect(() => {
        usize().then(res => setFirst(res.data.data === 0 ? true : false))
    }, [])

    const login = (e) => {
        e.preventDefault()
        signin(info).then(res => {
            localStorage.setItem("__token__", res.data.token)
            localStorage.setItem("__username__", info.username)
            axios.defaults.headers["Authorization"] = localStorage.getItem("__token__")
            nav(import.meta.env.BASE_URL+"/")
        })
    }

    const register = (e) => {
        e.preventDefault()
        signup(info).then(res => {
            console.log("token is ", res.data.token)
            localStorage.setItem("__token__", res.data.token)
            localStorage.setItem("__username__", info.username)
            axios.defaults.headers["Authorization"] = localStorage.getItem("__token__")
            nav(import.meta.env.BASE_URL+"/")
        })
    }

    return (
        <div className="login-page">
            <form onSubmit={e => { first ? register(e) : login(e) }}>
                <InputText autoFocus required value={info.username} onChange={(e) => setInfo({...info, username: e.target.value}) } placeholder="username" />
                <InputText type="password" required value={info.password} onChange={(e) => setInfo({...info, password: e.target.value}) } placeholder="password" />
                <Button type="submit" >{first ? "Signup" : "Signin"}</Button>
            </form>
        </div>
    )
}