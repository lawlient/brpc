const Entrance = require("../models/entrance")
const Quote = require("../models/quotes")


exports.get = async (req, res) => {
    const entrance = Entrance.find({})
    const quote = Quote.findOne({})

    const entrys = await entrance.exec();
    const quotes = await quote.exec();

    res.status(200).render("index", {quotes, entrys});
}
