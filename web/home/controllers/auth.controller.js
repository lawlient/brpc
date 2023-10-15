const config = require("../config/auth.config");
const db = require("../models");
const User = db.user;
const Role = db.role;

var jwt = require("jsonwebtoken");
var bcrypt = require("bcryptjs");

exports.index = (req, res) => {
    res.status(200).render("signup", {});
}
exports.login = (req, res) => {
    res.status(200).render("signin", {});
}

exports.signup = (req, res) => {
  const user = new User({
    username: req.body.username,
    password: bcrypt.hashSync(req.body.password, 8),
  });

  user.save().then(user => {
    if (req.body.roles) {
      Role.find(
        {
          name: { $in: req.body.roles },
        }).then(roles => {
          user.roles = roles.map((role) => role._id);
          user.save().then(user => {
            // res.send({ message: "User was registered successfully!" });
            res.body = JSON.stringify({
              username: user.name,
              password: req.body.password
            })
            res.redirect("/api/auth/login")
          }).catch(err => {
            if (err) {
              res.status(500).send({ message: err });
              return;
            }
          });
        }
      ).catch(err => {
          if (err) {
            res.status(500).send({ message: err });
            return;
          }
      });
    } else {
      Role.findOne({ name: "user" }).then((err, role) => {
        if (err) {
          res.status(500).send({ message: err });
          return;
        }

        user.roles = [role._id];
        user.save((err) => {
          if (err) {
            res.status(500).send({ message: err });
            return;
          }

          // res.send({ message: "User was registered successfully!" });
          res.status(200).render("signin", {})
        });
      });
    }
  }).catch(err => {
    if (err) {
      res.status(500).send({ message: err });
      return;
    }
  });
};

exports.signin = (req, res) => {
  console.log(req.body)
  User.findOne({
    username: req.body.username,
  })
    .populate("roles", "-__v")
    .exec().then(user => {
      if (!user) {
        return res.status(404).send({ message: "User Not found." });
      }

      var passwordIsValid = bcrypt.compareSync(
        req.body.password,
        user.password
      );

      if (!passwordIsValid) {
        return res.status(401).send({ message: "Invalid Password!" });
      }

      const token = jwt.sign({ id: user.id },
                              config.secret,
                              {
                                algorithm: 'HS256',
                                allowInsecureKeySizes: true,
                                expiresIn: 300, // 5 min
                              });

      var authorities = [];

      for (let i = 0; i < user.roles.length; i++) {
        authorities.push("ROLE_" + user.roles[i].name.toUpperCase());
      }

      req.session.token = token;

      /// res.status(200).send({
      ///   id: user._id,
      ///   username: user.username,
      ///   roles: authorities,
      /// });
      res.status(200).redirect("/entrance")
    }).catch(err => {
        res.status(500).send({ message: err });
        return;
    });
};

exports.signout = async (req, res) => {
  try {
    req.session = null;
    return res.status(200).send({ message: "You've been signed out!" });
  } catch (err) {
    this.next(err);
  }
};