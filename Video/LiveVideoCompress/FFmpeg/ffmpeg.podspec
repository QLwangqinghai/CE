Pod::Spec.new do |s|
  s.name         = "ffmpeg"
  s.version      = "1.0.1"
  s.summary      = "ffmpeg Libraries"
  s.description  = "ffmpeg ."
  s.homepage     = "https://github.com/bither/OpenSSL"
  s.license	     = { :type => 'OpenSSL (OpenSSL/SSLeay)', :file => 'LICENSE' }
  s.source       = { :git => "http://192.168.1.18/bichan_coms/Vendor.git", :branch => "master", }

  s.authors       =  {'x' => 'x'}
  
  s.ios.deployment_target = '9.0'

  s.source_files        = 'libs/**/*.{h, c}'
  s.public_header_files = 'libs/**/*.h'
  # s.header_dir          = 'openssl'
  s.vendored_libraries  = 'libs/**/*.a'
  s.libraries    = 'z', 'iconv',  'bz2'



end
