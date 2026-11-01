'use strict';
const server = require('../simulator/server');

module.exports = (req, res) => {
    server.emit('request', req, res);
};
