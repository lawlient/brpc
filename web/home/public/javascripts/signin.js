window.onload = () => {


const signin_button = document.querySelector("#Signin")
console.log(typeof(signin_button))
console.log(signin_button)
signin_button.addEventListener("click", () => {
    const name = document.querySelector("#name").value
    const psd = document.querySelector("#password").value
    let data = {}
    data.username = name
    data.password = psd
    console.log(data)
    signin(data)
})

async function signin(data) {
    let url = `/api/auth/signin`;
    const sin = await fetch(url, {
        method: "post",
        mode: 'cors',
        headers: {
            'Content-Type':'application/json'
        },
        body: JSON.stringify(data)
    });

    if (sin.status == 200) {
        window.location.replace("/entrance")
    }
}




}