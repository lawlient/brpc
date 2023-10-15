const input = document.querySelector("#entryinput")
const addbutton   = document.querySelector("#add")
const entrances = document.querySelectorAll(".entry")

// add a new entrance title to the list
addbutton.addEventListener("click", () => {
    add(input.value);
});

input.addEventListener("keydown", (event) => {
    if (event.key === "Enter")
    add(input.value)
})


document.querySelectorAll(".update").forEach((en, i) => {
    en.addEventListener("click", () => {
         //console.log(typeof(entrances[i]))
        console.log(entrances[i])
        title = entrances[i].querySelector(".title").textContent;
        desc  = entrances[i].querySelector(".desc").value;
        url   = entrances[i].querySelector(".url").value;
        console.log("desc is " + desc)
        console.log("url is " + url)
        update(title, desc, url)
    })
})

document.querySelectorAll(".delete").forEach((en, i) => {
    en.addEventListener("click", () => {
        delet(entrances[i].title)
    })
})

async function add(title) {
    await fetch("/entrance/add", {
        method: "post",
        mode: "cors",
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({"title": title})
    });
    location.reload();
}

async function update(title, desc, url) {
    await fetch("/entrance/update", {
        method: "post",
        mode: "cors",
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            "title": title,
            "description": desc,
            "url": url,
        })
    });
    location.reload();
}

async function delet(title) {
    await fetch("/entrance/delete", {
        method: "post",
        mode: "cors",
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            "title": title,
        })
    });
    location.reload();
}

