import React from 'react'
import { useSelector } from 'react-redux'
import { css } from 'glamor'

import heart from '../../static/img/heart.svg'
import diamond from '../../static/img/diamond.svg'
import skull from '../../static/img/skull.svg'
import eye from '../../static/img/eye.svg'
import wave from '../../static/img/wave.svg'

export default ({ style, id, label, setCategory, isActive }) => {
	const theme = useSelector(state => state.theme) || []
	return (
		<div
			{...css({
				borderRadius: theme.borderRadius,
				backgroundRepeat: 'no-repeat',
				backgroundPosition: '1rem .25rem',
				backgroundSize: 'contain',
				'&:hover': {
					backgroundColor: 'rgba(255, 255, 255, .4)',
				},
				'&:active': {
					filter: 'invert(100%)',
				},
				backgroundColor: `rgba(255, 255, 255, ${isActive ? 0.2 : 0})`,
				'&.heart': { backgroundImage: `url(${heart})` },
				'&.fortune': { backgroundImage: `url(${diamond})` },
				'&.health': { backgroundImage: `url(${skull})` },
				'&.self': { backgroundImage: `url(${eye})` },
				'&.wave': { backgroundImage: `url(${wave})` },
			})}
			{...style}
			className={id}>
			<input type="radio" id={id} name="category" value={id} style={{ display: 'none' }} />
			<label
				onClick={e => {
					setCategory(e.target.htmlFor)
				}}
				htmlFor={id}
				{...css({
					fontSize: '1.5rem',
					margin: '.25rem 0',
					display: 'block',
					padding: '.25rem 1rem .25rem 3rem',
					color: '#000',
					[`@media(min-width: ${theme.breaks.md}px)`]: {
						fontSize: '2.5rem',
						paddingLeft: '5.25rem',
					},
				})}>
				{label}
			</label>
		</div>
	)
}
