const express = require('express');
const router = express.Router();

const home = require("../controllers/indexController")

/* GET home page. */
router.get('/', home.get);

module.exports = router;
