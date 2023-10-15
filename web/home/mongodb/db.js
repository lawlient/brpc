const db = require("../models");
const dbconfig = require("../config/db.config");
const Role = db.role;


exports.dbconnect = () => {
  const url = `mongodb://${dbconfig.HOST}:${dbconfig.PORT}/${dbconfig.DB}`;
  db.mongoose.connect(url, { useNewUrlParser: true, useUnifiedTopology: true })
  .then(() => {
    console.log("Successfully connect to MongoDB.");
    initial();
  })
  .catch(err => {
    console.error("Connection error", err);
    process.exit();
  });

}

function initial() {
  Role.estimatedDocumentCount().then(count => {
    if (count === 0) {
      add_role("user");
      add_role("moderator");
      add_role("admin");
    }
  }).catch(err => {
      console.log("estimate document count fail" + err)
  });
}

function add_role(name_) {
  new Role({
    name: name_
  }).save().then(err => {
    console.log("added '" + name_ + "' to roles collection");
  }).catch(err => {
    if (err) {
      console.log("error", err);
    }
  });
}