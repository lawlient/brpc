import axios from 'axios';

var HOSTNAME = import.meta.env.VITE_BASE_URL
var BASEPATH = import.meta.env.BASE_URL
axios.defaults.baseURL = "https://jovan.vip.cpolar.cn/xpansync"
axios.defaults.headers["Authorization"] = localStorage.getItem("__token__")


export function get_setting() {
    return axios.post("/setting")
}

export function set_setting({key, value, desc}) {
    return axios.post("/setting/update", {
        key: key,
        value: value
    })
}

export function userinfo() {
    return axios.post("/user")
}

export function quotainfo() {
    return axios.post("/quota")
}


export function getcode() { return axios.post("/code") }
export function gettoken() { return axios.get("/token") }
export function getquota() { return axios.post("/quota") }

export function newtoken(code) { 
    return axios.post("/token/update", {
        Code: code,
    }) 
}

export function tasklist() { return axios.post("/tasks") }
export function addtask(t) { return axios.post("/tasks/add", t) }
export function deltask(t) { return axios.post("/tasks/del", t) }
export function updtask(t) { return axios.post("/tasks/upd", t) }