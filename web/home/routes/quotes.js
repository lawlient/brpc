var express = require('express');
var router = express.Router();
const quote = require("../controllers/quoteController")

/* GET quotes. */
router.get('/', quote.get);

router.post('/add', quote.add);
router.post('/update', quote.update);
router.post('/delete', quote.delete);

module.exports = router;

