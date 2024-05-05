import axios from 'axios';

var HOSTNAME = import.meta.env.VITE_BASE_URL || ""
var BASEPATH = import.meta.env.BASE_URL
axios.defaults.baseURL = HOSTNAME + BASEPATH
axios.defaults.headers["Authorization"] = localStorage.getItem("__token__")

axios.interceptors.response.use((response) => {
    return response;
}, (error) => { // Anything except 2XX goes to here
    const status = error.response?.status || 500;
    if (status === 401 || status === 403) {
        window.location =  BASEPATH + "signin"
    } else {
        return Promise.reject(error); // Delegate error to calling side
    }   
});

export function get_setting() {
    return axios.post("api/setting")
}

export function set_setting({key, value, desc}) {
    return axios.post("api/setting/update", {
        key: key,
        value: value
    })
}

export function userinfo() {
    return axios.post("api/user")
}

export function quotainfo() {
    return axios.post("api/quota")
}


export function getcode() { return axios.post("api/code") }
export function gettoken() { return axios.get("api/token") }
export function getquota() { return axios.post("api/quota") }

export function newtoken(code) { 
    return axios.post("api/token/update", {
        Code: code,
    }) 
}

export function tasklist() { return axios.post("api/tasks") }
export function addtask(t) { return axios.post("api/tasks/add", t) }
export function deltask(t) { return axios.post("api/tasks/del", t) }
export function updtask(t) { return axios.post("api/tasks/upd", t) }



export function signin(u) { return axios.post("auth/login", u) }
export function signup(u) { return axios.post("auth/signup", u) }

export function usize() { return axios.post("api/usersize") }
export function userlist() { return axios.post("api/userlist") }
export function getuser(u) { return axios.post("api/user/", u) }
export function upduser(u) { return axios.post("api/user/upd", u)}