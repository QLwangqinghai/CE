Pod::Spec.new do |s|
  s.name             = 'SITP'
  s.version          = '0.1.0'
  s.summary          = 'SITP.'

  s.description      = 'SITP.'

  s.homepage         = 'https://github.com/QLwangqinghai/CE.git'
  s.license  = { :type => 'MIT'}
  s.author           = { '' => '' }
  s.source           = { :git => 'https://github.com/QLwangqinghai/CE.git', :tag => "#{s.version}"}

  s.ios.deployment_target = '9.0'

  s.source_files = "*.h", "Source/**/*.{h,c,m,cpp,swift}"

  s.public_header_files = "Source/**/*.h", "*.h"

  s.dependency 'CCFoundation'

end
