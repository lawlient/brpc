window.onload = () => {
    const ens = document.querySelectorAll(".entry")
    ens.forEach((en, i) => {
        en.addEventListener("click", () => {
            console.log(ens[i].querySelector(".dsc").href);
            window.open(ens[i].querySelector(".dsc").href);
        })
    })

    const home = document.querySelector("#logo")
    const admin = document.querySelector("#admin")

    home.addEventListener("click", () => {
        location.replace("/");
    })

    admin.addEventListener("click", () => {
        window.location.replace("/entrance")
    })
}