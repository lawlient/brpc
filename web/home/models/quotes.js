const mongoose = require("mongoose");

const QuoteSchema = {
    q: {
        type: String,
        required: true
    },
    author: {
        type: String,
        required: true
    }
};

module.exports = mongoose.model("Quote", QuoteSchema);