const signup_button = document.querySelector("#Signup")


signup_button.addEventListener("click", () => {
    const name = document.querySelector("#name").value
    const psd = document.querySelector("#password").value
    const roles = document.querySelector("#role").value
    var data = {}
    data.username = name
    data.password = psd
    data.roles = [roles]
    console.log(data)
    signup(data)
})


async function signup(data) {
    let url = `/api/auth/signup`
    const res = await fetch(url, {
        method: "post",
        mode: 'cors',
        headers: {
            'Content-Type':'application/json'
        },
        body: JSON.stringify(data)
    });
    console.log(res);
    if (res.status == 200 || res.status == 302) {
        window.open(res.url)
    }
}