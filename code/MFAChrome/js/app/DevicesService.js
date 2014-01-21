app.factory('devicesService', function () {
    var devices = {};

    return {
        saveDevices:function (data) {
            devices = data;
        },
        getDevices:function () {
            return devices;
        }
    };
});