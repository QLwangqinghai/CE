Pod::Spec.new do |s|
  s.name             = 'CoreEventShim'
  s.version          = '0.1.0'
  s.summary          = 'CoreEventShim.'

  s.description      = 'CoreEventShim.'

  s.homepage         = 'https://github.com/QLwangqinghai/CE.git'
  s.license  = { :type => 'MIT'}
  s.author           = { '' => '' }
  s.source           = { :git => 'https://github.com/QLwangqinghai/CE.git', :tag => "#{s.version}"}

  s.ios.deployment_target = '9.0'
  s.osx.deployment_target = "10.10"

  s.source_files = "*.h", "Source/**/*.{h,c,m,cpp,swift}"
  s.public_header_files = "Source/*.h", "*.h"

end