module.exports = {
  devServer: {
    proxy: {
      '^/client': {
        target: 'http://localhost:8888',
        changeOrigin: true
      },
    }
  }
}
