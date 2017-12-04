if build_client == true
    install_data('org.alunux.usbipapp.pkexec.policy', install_dir : join_paths(get_option('datadir'), 'polkit-1/actions'))
    install_data('usbip-app-client.desktop', install_dir : join_paths(get_option('datadir'), 'applications'))
endif

if build_server == true
    install_data('usbip-server.service', install_dir : join_paths(get_option('sysconfdir'), 'systemd/system'))
endif