Pod::Spec.new do |s|
  s.name     = 'BCNetworkSDK'
  s.version  = '0.0.13'
  s.license  = { :type => 'MIT'}
  s.summary  = 'BCNetworkSDK'
  s.homepage = 'http://192.168.1.18/bichan/BCNetworkSDK-iOS'
  s.social_media_url = ''
  s.authors  = { '' => '' }
  s.source   = { :git => 'http://192.168.1.18/bichan/BCNetworkSDK-iOS', :submodules => true, :tag => "#{s.version}"}
  s.requires_arc = true

  s.platform     = :ios
  s.ios.deployment_target = '8.0'

  s.public_header_files = 'BCNetworkSDK/BCNetworkSDK/BCNetworkSDK.h'
  s.source_files = 'BCNetworkSDK/BCNetworkSDK/BCNetworkSDK.h'

  s.subspec 'Model' do |ss|
    ss.source_files = 'BCNetworkSDK/BCNetworkSDK/Source/Model/*.{h,m}'
    ss.public_header_files = 'BCNetworkSDK/BCNetworkSDK/Source/Model/*.h'
  end

  s.subspec 'Request' do |ss|
    ss.source_files = 'BCNetworkSDK/BCNetworkSDK/Source/Request/*.{h,m}'
    ss.public_header_files = 'BCNetworkSDK/BCNetworkSDK/Source/Request/*.h'
    ss.libraries    = "z"
    ss.frameworks = 'Security'
    ss.dependency 'AFNetworking', '=3.1.0'
  end
  s.subspec 'Base' do |ss|
    ss.source_files = 'BCNetworkSDK/BCNetworkSDK/Source/Base/**/*.{h,m}'
    ss.public_header_files = 'BCNetworkSDK/BCNetworkSDK/Source/Base/**/*.h'
    ss.dependency 'CocoaLumberjack', '>=1.9.1'

  end
  s.dependency 'CocoaLumberjack', '>=1.9.1'

end
