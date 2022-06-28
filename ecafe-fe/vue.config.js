module.exports = {
  devServer: {
    proxy: {
      '^/client': {
        target: 'http://127.0.0.1:8888',
        changeOrigin: true
      },
    }
  }
}
