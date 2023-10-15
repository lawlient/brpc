const express = require('express');
const { authJwt } = require("../middlewares");
const router = express.Router();
const entry = require("../controllers/entryController")

router.get("/", [authJwt.verifyToken, authJwt.isAdmin], entry.get);
router.post("/add", [authJwt.verifyToken, authJwt.isAdmin], entry.add);
router.post("/update", [authJwt.verifyToken, authJwt.isAdmin], entry.update);
router.post("/delete", [authJwt.verifyToken, authJwt.isAdmin], entry.delete);

module.exports = router;