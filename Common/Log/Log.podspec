Pod::Spec.new do |s|
  s.name             = 'Log'
  s.version          = '0.1.0'
  s.summary          = 'Log.'

  s.description      = 'Log.'

  s.homepage         = 'https://github.com/QLwangqinghai/CE.git'
  s.license  = { :type => 'MIT'}
  s.author           = { '' => '' }
  s.source           = { :git => 'https://github.com/QLwangqinghai/CE.git', :tag => "#{s.version}"}

  s.ios.deployment_target = '9.0'
  s.osx.deployment_target = "10.14"

  s.source_files = "*.h", "Sources/**/*.{h,c,m,cpp,swift}"

end
