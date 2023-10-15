const Quote = require("../models/quotes");

exports.get = async (req, res) => {
    const quote = Quote.findOne({})

    const quotes = await quote.exec();

    res.redirect("/");
}


exports.add = async (req, res) => {
    const item = new Quote({
        q : 'Stay hungry, stay foolish.',
        author : 'Steven Jobs',
    })
    item.save()
    res.status(200).redirect("/quotes/")
}


exports.update = async (req, res) => {
    const item = await Quote.find({
        q : 'Stay hungry, stay foolish.',
        author : 'Steven Jobs',
    })

    if(!item){
        res.status(400)
        throw new Error("Item not found")
    }
    res.status(200).redirect("/quotes/")
}


exports.delete = async (req, res) => {
    const item = new Quote({
        q : 'Stay hungry, stay foolish.',
        author : 'Steven Jobs',
    })
    item.save()
    res.status(200).redirect("/quotes/")
}

