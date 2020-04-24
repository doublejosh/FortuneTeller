import React, { useEffect } from 'react'
import { useLocation } from 'react-router-dom'
import { useSelector } from 'react-redux'
import Menu from './Menu'
import { css } from 'glamor'
import packageJson from '../../package.json'

const ScrollToTop = () => {
	const { pathname } = useLocation()

	useEffect(() => {
		window.scrollTo(0, 0)
	}, [pathname])

	return null
}

export default () => {
	const theme = useSelector(state => state.theme) || []
	return (
		<>
			<ScrollToTop />
			<p
				{...css({
					position: 'absolute',
					top: '-.75rem',
					left: '.5rem',
					[`@media(min-width: ${theme.breaks.md}px)`]: {
						top: '-1.25rem',
					},
					opacity: 0.3,
					'&:hover': { opacity: 1 },
				})}>
				{packageJson.version}
			</p>
			<Menu />
		</>
	)
}
