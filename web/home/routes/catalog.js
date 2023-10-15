const express = require("express");
const router = express.Router();

// 导入控制器模块
// const book_controller = require("../controllers/bookController");
const author_controller = require("../controllers/authorController");
// const genre_controller = require("../controllers/genreController");
// const book_instance_controller = require("../controllers/bookinstanceController");

/// 藏书路由 ///

// GET 获取藏书编目主页
// router.get("/", book_controller.index);

// GET 请求添加新的藏书。注意此项必须位于显示藏书的路由（使用了 id）之前。
router.get("/author/", author_controller.author_list);


/// 藏书副本、藏书种类、作者的路由与藏书路由结构基本一致，只是无需获取主页 ///

module.exports = router;
