const mongoose = require("mongoose");

const EntranceSchema = {
    title: {
        type: String,
        required: true
    },
    description: {
        type: String,
        required: true
    },
    url: String
};

module.exports = mongoose.model("Entrance", EntranceSchema);