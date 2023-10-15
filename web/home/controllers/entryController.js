const Entrance = require("../models/entrance");

exports.get = async (req, res) => {
    const entrance = Entrance.find({})
    const entrys = await entrance.exec();

    res.status(200).render("entry", {entrys})
}


exports.add = async (req, res) => {
    const entry = new Entrance({
        title : req.query.title,
        description : req.query.title,
        url: '',
    })
    entry.save()
    res.status(200).redirect("/entrance/")
}


exports.update = async (req, res) => {
    Entrance.findOne({
        title: req.body.title,
    }).exec().then(item => {
        item.url = req.body.url
        item.description = req.body.description
        item.title = req.body.title

        item.save().then(item => {

        }).catch(err => {
            console.log(err)
        })
    }).catch(err => {
        console.log(err)
    })
}


exports.delete = async (req, res) => {
    const item = new Entrance({
        title : req.query.title
    })
    item.delete()
    res.status(200).redirect("/entrance/")
}

